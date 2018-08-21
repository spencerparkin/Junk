// ga_system.jsx

class GAResults extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            results_array: []
        }
    }

    OnXButtonClicked(evt) {
        let id = evt.target.parentElement.id;
        let match = /xbutton_([0-9]+)/g.exec(id);
        let i = parseInt(match[1]);
        let results_array = this.state.results_array.slice();
        results_array.splice(i, 1);
        this.setState({results_array: results_array});
    }

    OnMathImageClicked(text) {
        let current_text = $('#calculator_input').val();
        $('#calculator_input').val(current_text + text);
    }

    render() {
        let div_array = [];
        for(let i = 0; i < this.state.results_array.length; i++) {
            let result_entry = this.state.results_array[i];
            let div_input = (
                <div className="results_scroll_box_item results_scroll_box_input">
                    <img src={result_entry.input_image_url} style={{'cursor': 'pointer'}} onClick={this.OnMathImageClicked.bind(this, result_entry.input_text)}></img>
                    <span id={'xbutton_'+i.toString()} className="results_scroll_box_input_xbutton"><img src="images/xbutton.png" onClick={this.OnXButtonClicked.bind(this)}></img></span>
                </div>
            );
            let div_output = (
                <div className="results_scroll_box_item results_scroll_box_output">
                    <img src={result_entry.output_image_url} style={{'cursor': 'pointer'}} onClick={this.OnMathImageClicked.bind(this, result_entry.output_text)}></img>
                </div>
            );
            div_array.push(div_input);
            div_array.push(div_output);
        }
        return React.createElement('div', {id: 'results_scroll_box'}, ...div_array);
    }

    componentDidUpdate() {
        // This is a lame hack.  Wait for the images to load before we try
        // to use the scroll box's height in the scroll calculation.
        setTimeout(() => {
            let scroll_box = document.getElementById('results_scroll_box');
            scroll_box.scrollTop = scroll_box.scrollHeight;
        }, 500);
    }
}

var ga_results = ReactDOM.render(<GAResults/>, document.getElementById('ga_results'));

var OnCalculatorInputEnterPressed = function(ele) {
    if(event.key === 'Enter') {
        let data = {
            input_text: ele.value
            // TODO: Add GA signature here.
        }
        $('#ajax_wait').show();
        // TODO: Use a promise here instead?
        $.ajax({
            url: 'calculate',
            data: JSON.stringify(data),
            contentType: 'application/json',
            type: 'POST',
            success: (json_data) => {
                $('#ajax_wait').hide();
                if(json_data.error) {
                    alert(json_data.error);
                } else {
                    let result_entry = {
                        input_text: ele.value,
                        output_text: json_data.output_text,
                        input_image_url: json_data.input_image_url,
                        output_image_url: json_data.output_image_url
                    }
                    let results_array = ga_results.state.results_array.slice();
                    results_array.push(result_entry);
                    // TODO: If array is too big, start deleting oldest entries?
                    ga_results.setState({results_array: results_array});
                    ele.value = '';
                }
            }
        });
    }
}