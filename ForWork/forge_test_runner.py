# forge_test_runner.py

import time
import json

class ForgeTestRunner(object):
    # This class is designed to run in the front-end or the back-end.
    # It is also designed to scale horizontally.

    def __init__(self, config=None):
        if config is None:
            with open('forge_config.json', 'r') as handle:
                self.config = json.reads(handle.read())
        else:
            self.config = config

        self._action_state_handler_map = {
            'allocating_resources': self._allocate_resources,
            'running': self._run_test,
            'deallocating_resources': self._deallocate_resources,
        }

    def run(self):
        while True:

            while self._token_found():
                self._process_state_transition('initial', 'allocating_resources')
                self._process_state_transition('ready_to_run', 'running')
                self._process_state_transition('complete', 'deallocating_resources')
                self._process_state_transition('final', 'deleting')
                self._process_state_transition('waiting_for_part_2', 'allocating_resources')

            if config.get('exit_if_token_not_found', False):
                break
            else:
                time.sleep(10)

    def _token_found(self):
        with ForgeMongoClient(self.config['mongoserver_tests']) as client:
            tests_database = client['forge_tests']
            test_work_coll = tests_database['test_work']
            test_work = test_work_coll.find({'token': self.config.get('token')})
            return True if test_work is not None else False

    def _process_state_transition(self, rest_state, action_state):
        with ForgeMongoClient(self.config['mongoserver_tests']) as client:
            tests_database = client['forge_tests']
            test_work_coll = tests_database['test_work']
            key = {'state': rest_state, 'token': self.config.get('token')}
            test_work = test_work_coll.find_one(key)
            if test_work is not None:
                update = {'$set': {'state': action_state, 'state_time': datetime.datetime.now()}}
                key = {'_id': test_work['_id']}
                result = test_work_coll.update_one(key, update)
                if result.modified_count == 1:
                    try:
                        # We are the only instance that transitioned the test work into the action state.
                        target_state, action_updates = self._action_state_handler_map[action_state](test_work, client)

                    except Exception as ex:
                        # If an exception occurred, move us back to the rest state.
                        update = {
                            '$set': {
                                'state': rest_state,
                                'state_time': datetime.datetime.now(),
                                'state_exception': str(ex)
                            }
                        }
                        test_work_coll.update_one(key, update)

                    else:
                        # If an exception did not occur, move us to the target state.
                        if target_state is not None:
                            update = {
                                '$set': {
                                    **action_updates,
                                    'state': target_state,
                                    'state_time': datetime.datetime.now(),
                                    'state_exception': None
                                }
                            }
                            test_work_coll.update_one(key, update)
                        else:
                            test_work_coll.delete_one(key)

    def _allocate_resources(self, test_work, client):

        #...
        # if we fail to allocate resources, then we throw an exception
        with self._lock_document(host_doc, hosts_coll, client):
            pass

        return 'ready_to_run', {}

    def _deallocate_resources(self, test_work, client):

        with self._lock_document(host_doc, hosts_coll, client):
            pass

        if test_work.get('needs_part_2', False):
            return 'waiting_for_part_2', {}
        else:
            return 'final', {}

    def _run_test(self, test_work, client):
        pass

if __name__ == '__main__':
    runner = ForgeTestRunner()
    runner.run()