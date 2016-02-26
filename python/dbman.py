#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import os
import plyvel
import hashlib
from datetime import datetime
from json import *

root_path = os.getcwd()+'/datas'
user_db = root_path + '/users.db'
log_db = root_path + '/logs.db'
privilage_db = root_path + '/priv.db'
IRs_db = root_path + '/IRs.db'
#"IRID":"{name:NickName,room:RoomID,sig:IR_Signal}"
def check_dbs():
    plyvel.DB(user_db, create_if_missing=True)
    plyvel.DB(log_db, create_if_missing=True)
    plyvel.DB(privilage_db, create_if_missing=True)
    plyvel.DB(IRs_db, create_if_missing=True)
def reset_admin_user():
    db = plyvel.DB(user_db, create_if_missing=True)
    password = 'admin'
    password = hashlib.sha1(password).hexdigest();
    password = hashlib.sha1(password).hexdigest();
    db.put('admin', password)
    db = plyvel.DB(privilage_db, create_if_missing=True)
    db.put('admin', 'admin')
def verify_user(username,password):
    db = plyvel.DB(user_db)
    pw_chk = None
    pwd = hashlib.sha1(password).hexdigest();
    pw_chk = db.get(username.encode('utf8'))
    if pw_chk == None:
        return 'User not exist.'
    if pwd == pw_chk:
        return 'Success.'
    return 'Password wrong.'
def get_privilage(username):
    db = plyvel.DB(privilage_db)
    res = db.get(username.encode('utf8'))
    if res == None:
        res = "User error: No privilage."
    return res
def create_user(username,password):
    db = plyvel.DB(user_db)
    res = db.get(username.encode('utf8'))
    if res == None:
        pwd = hashlib.sha1(password).hexdigest();
        db.put(username,pwd)
        return 'Success.'
    else:
        return res
def get_user_count():
    db = plyvel.DB(user_db)
    count = 0
    for key, value in db:
        count = count+1
    return count
def get_IRs_count():
    db = plyvel.DB(IRs_db)
    count = 0
    for key, value in db:
        count = count+1
    return count
def make_log(log):
    db = plyvel.DB(log_db)
    time = str(datetime.utcnow())
    db.put(time,log)
def get_log(time):
    db = plyvel.DB(log_db)
    log = db.get(time.encode('utf8'))
    if log != None:
        return log
    else:
        return 'Log not exist.'
def get_log_range(time_start,time_end):
    db = plyvel.DB(log_db)
    res_dict = {}
    for key, value in db.iterator(start=time_start, stop=time_end):
        res_dict[key]=value
    return res_dict
def get_IR_dict(start,num):
    db = plyvel.DB(IRs_db)
    i = 0
    res_dict = {}
    it = db.iterator(include_value=False)
    if start != None:
        it.seek(start)
    else:
        it.seek_to_start()
    while True:
        try:
            k = next(it)
            tmp_d=json.loads(db.get(k))
            res_dict[k]=tmp_d["name"]
            i=i+1
            if (i > num):
                break
        except StopIteration as err:
            break
    return res_dict
def write_database(dbname,key,value):
    db = plyvel.DB(root_path + dbname)
    db.put(key,value)
def read_database(dbname,key):
    db = plyvel.DB(root_path + dbname)
    res = db.get(key)
    return res
def get_user_list(start,num):
    db = plyvel.DB(user_db)
    i = 0
    res_dict = {}
    it = db.iterator(include_value=False)
    if start != "None":
        it.seek(start)
    else:
        it.seek_to_start()
    while True:
        try:
            k = next(it)
            res_dict[i]=k
            i=i+1
            if (i > num):
                break
        except StopIteration as err:
            break
    return res_dict
def get_priv_list(start,num):
    db = plyvel.DB(privilage_db)
    i = 0
    res_dict = {}
    it = db.iterator(include_value=False)
    if start != "None":
        it.seek(start)
    else:
        it.seek_to_start()
    while True:
        try:
            k = next(it)
            res_dict[k] = db.get(k)
            i=i+1
            if (i > num):
                break
        except StopIteration as err:
            break
    return res_dict
def change_user_pass(username,new_password):
    db = plyvel.DB(user_db)
    res = db.get(username.encode('utf8'))
    if (res == None):
        return "User not exist."
    new_password = hashlib.sha1(new_password).hexdigest();
    db.put(username,new_password)
    return "Success."
def write_IR_detail(IRID,det):
    db = plyvel.DB(IRs_db)
    data = db.get(str(IRID))
    if (data == None):
        return "IRID not exist."
    m_dict = json.loads(data)
    m_dict["sig"] = det
    tmp = JSONEncoder().encode(m_dict)
    db.put(str(IRID),tmp)
    return "Success."
def get_user(username):
    db = plyvel.DB(user_db)
    user = db.get(username.encode('utf8'))
    if (user == None):
        return -1
    return 0
def IR_Remove(IRID):
    db = plyvel.DB(IRs_db)
    res = db.get(str(IRID))
    if(res != None):
        pass
    return 0
def IR_modify(IRID,DATAS):
    return 0
