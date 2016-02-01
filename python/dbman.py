#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import os
import leveldb
import hashlib
from datetime import datetime

root_path = os.getcwd()+'/datas'
user_db = root_path + '/users.db'
log_db = root_path + '/logs.db'
privilage_db = root_path + '/priv.db'
IRs_db = root_path + '/IRs.db'
def create_NewDB():
    leveldb.LevelDB(log_db)
    leveldb.LevelDB(privilage_db)
    leveldb.LevelDB(IRs_db)
    #Admin User
    db = leveldb.LevelDB(user_db)
    password = 'admin'
    password = hashlib.sha1(password).hexdigest();
    password = hashlib.sha1(password).hexdigest();
    db.Put('admin', password)
    db = leveldb.LevelDB(privilage_db)
    db.Put('admin', 'admin')
    return 'Success.'
def verify_user(username,password):
    db = leveldb.LevelDB(user_db)
    pw_chk = None
    pwd = hashlib.sha1(password).hexdigest();
    try:
        pw_chk = db.Get(username)
    except KeyError as err:
        return 'User not exist.'
    if pwd == pw_chk:
        return 'Success.'
    return 'Password wrong.'
def get_privilage(username):
    db = leveldb.LevelDB(privilage_db)
    try:
        res = db.Get(username)
    except KeyError as err:
        res = "User error: No privilage."
        print res
    return res
def create_user(username,password):
    db = leveldb.LevelDB(user_db)
    try:
        res = db.Get(username)
    except KeyError as err:
        pwd = hashlib.sha1(password).hexdigest();
        db.Put(username,pwd)
        return 'Success.'
    return res #user existed
def get_user_count():
    db = leveldb.LevelDB(user_db)
    iter = leveldb.Iterator(db)
    iter.First()
    count = 0
    while True:
        if (iter.Validate()):
            count = count +1
            iter.Next()
        else:
            break
    return count
def get_IRs_count():
    db = leveldb.LevelDB(IRs_db)
    iter = leveldb.Iterator(db)
    iter.First()
    count = 0
    while True:
        if (iter.Validate()):
            count = count +1
            iter.Next()
        else:
            break
    return count
def make_log(log):
    db = leveldb.LevelDB(log_db)
    time = str(datetime.utcnow())
    db.Put(time,log)
def get_log(time):
    db = leveldb.LevelDB(log_db)
    try:
        log = db.Get(time)
        return log
    except KeyError as err:
        return 'Log not exist.'
def get_log_range(time_start,time_end):
    db = leveldb.LevelDB(log_db)
    res_list = list(db.RangeIter(key_from = time_start, key_to = time_end))
    return res_list
def check_dbs():
    if os.path.exists(user_db):
        if os.path.exists(log_db):
            if os.path.exists(privilage_db):
                if os.path.exists(IRs_db):
                    return True
