#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import mpipe,time,Queue
import thread
from json import *

pipe_tfd = 0
pipe_rfd = 0
pacID = 0
Read_queue = Queue.Queue()

def m_write_dict(data_dict):
    tmp=data_dict
    tmp['I'] = gene_pacID()
    tmp['T'] = time.time()
    return m_write(JSONEncoder().encode(data_dict))
def m_write(string):
    return mpipe.writepipe(pipe_tfd,string)
def m_read():
    return mpipe.readpipe(pipe_rfd)
def get_online_client():
    return 0
def get_ready_info():
    return 0
def gene_pacID():
    pacID = pacID + 1
    return pacID
def clean_pacID_list():
    #add clean condition;
    pacID = 0;
    return pacID
def read_pipe_thread():
    while(1):
        Read_queue.put(m_read())
