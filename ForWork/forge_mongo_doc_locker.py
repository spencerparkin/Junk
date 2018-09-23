# forge_mongo_doc_locker

import datetime
import socket
import time

class ForgeMongoDocLocker(object):
    # It's best to do atomic operations on a mongo document using a conditional update of the document.
    # However, if you want a sequence of logic, analysis and updates of the document to become an atomic
    # operation on the document, and you don't know how to fit that into a single conditional update query,
    # then this class may be useful to you.

    def __init__(self, mongo_coll, mongo_doc_id, timeout=5):
        self.mongo_coll = mongo_coll
        self.mongo_doc_id = mongo_doc_id
        self.timeout = timeout

    def __enter__(self):
        start_time = datetime.datetime.now()
        while True:
            key = {
                '_id': mongo_doc_id,
                '$or': [
                    {
                        'doc_lock': {'$exists': False}
                    },
                    {
                        'doc_lock.locked_by': None
                    },
                    {
                        # This is critical to ensuring that no document is accidentally left in the lock state forever.
                        # What we're saying here is that a doc can be acquired in the case that it has been locked for too long.
                        # Need to use ISODate()?
                        '$where': 'function() {try{return (new Date() - this.doc_lock.lock_time) > this.doc_lock.timeout;} catch(e) {return false;}}'
                    }
                ]
            }
            update = {
                '$set': {
                    'doc_lock': {
                        'locked_by': socket.gethostname(),
                        'lock_time': datetime.datetime.now(),
                        'timeout': 10 * 1000
                    }
                }
            }
            result = self.mongo_coll.update_one(key, update)
            if result.modified_count == 1:
                mongo_doc = self.mong_coll.find_one({'_id': self.mongo_doc_id})
                return mongo_doc
            time.sleep(1)
            current_time = datetime.datetime.now()
            elapsed_time = current_time - start_time
            if elapsed_time.total_seconds() > self.timeout:
                raise Exception('Failed to acquire mongo doc lock.')

    def __exit__(self, type, value, traceback):
        key = {
            '_id': self.mongo_doc_id,
            'doc_lock.locked_by': socket.gethostname()
        }
        update = {
            '$set': {
                'dock_lock': {
                    'locked_by': None,
                }
            }
        }
        self.mongo_coll.update_one(key, update)