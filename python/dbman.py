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

root_path = os.getcwd()+'/datas'
user_db = root_path + '/users.db'
log_db = root_path + '/logs.db'
privilage_db = root_path + '/priv.db'
IRs_db = root_path + '/IRs.db'
def create_NewDB():
    leveldb.LevelDB(user_db)
    leveldb.LevelDB(log_db)
    leveldb.LevelDB(privilage_db)
    leveldb.LevelDB(IRs_db)
    return 'Success.'
def verify_user(username,password):
    db = leveldb.LevelDB(user_db)
    pw_chk = None
    try:
        pw_chk = db.Get(username)
    except KeyError as err:
        return 'User not exist.'
    if password == pw_chk:
        return 'Success.'
    return 'Password wrong.'
def get_privilage(username):
    db = leveldb.LevelDB(privilage_db)
    try:
        res = db.Get(username)
    except KeyError as err:
        res = "User error:" + err
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
def get_sensors_count():
    return 0
