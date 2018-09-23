# forge_test_runner.py

import time
import json

from forge_sys.forge_misc.forge_mongo_doc_locker import ForgeMongoDocLocker

class ForgeTestRunner(object):
    # This class is designed to run in the front-end or the back-end.
    # It is also designed to scale horizontally.

    def __init__(self, config=None):
        if config is None:
            with open('forge_config.json', 'r') as handle:
                self.config = json.reads(handle.read())
        else:
            self.config = config

    def run(self):
        while True:
            while self._token_found():
                self._process_test_work()
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

    def _process_test_work(self):
        with ForgeMongoClient(self.config['mongoserver_tests']) as client:
            tests_database = client['forge_tests']
            test_work_coll = tests_database['test_work']
            key = {'token': self.config.get('token')}
            test_work = test_work_coll.find_one(key)
            if test_work is not None:
                update = {'$set': {'captured_by': socket.gethostname(), 'capture_time': datetime.datetime.now()}}
                key = {'_id': test_work['_id'], 'captured_by': None}
                result = test_work_coll.update_one(key, update)
                if result.modified_count == 1: # One and only one test runner instance will modify the test work document.
                    key = {'_id': test_work['_id']}
                    try:
                        test_work = test_work_coll.find_one(key)
                        self._process_test_work_internal(test_work, client)
                    except Exception as ex:
                        update = {'$set': {'captured_by': None, 'capture_time': None, 'test_runner_exception': str(ex)}}
                        test_work_coll.update_one(key, update)
                    else:
                        test_work_coll.delete_one(key)

    def _process_test_work_internal(self, test_work, client):

        # Find a host against which to run the test.
        hosts_coll_name, host_doc_id = self._find_host_for_test_work(test_work, client)
        hosts_coll = client['forge_servers'][hosts_coll_name]

        # Allocate resources used by the test.  To prevent resource leaking,
        # every kind of resource should have a time-out.
        resource_list = self._determine_resource_requirements_of_test_work(test_work)
        with ForgeMongoDocLocker(hosts_coll, host_doc_id) as host_doc:
            if not all([resource.can_be_used(host_doc) for resource in resource_list]):
                raise Exception('Cannot allocate all needed resources for test work at this time.')
            for resource in resource_list:
                resource.allocate(hosts_coll, host_doc)

        try:
            self._run_test(test_work)

        finally:

            # Deallocate resources used by the test.
            with ForgeMongoDocLocker(hosts_coll, host_doc_id) as host_doc:
                for resource in resource_list:
                    resource.deallocate(hosts_coll, host_doc)

    def _find_host_for_test_work(self, test_work, client):
        pass
        # Raise exception if we don't find a host that's ready.

    def _determine_resource_requirements_of_test_work(self, test_work):
        pass

    def _run_test(self, test_work, client):
        pass

    def _fabricate_test_work(self):
        pass

if __name__ == '__main__':
    runner = ForgeTestRunner()
    runner.run()