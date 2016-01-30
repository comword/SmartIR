#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import mpipe
pipefd = 0

def m_write(string):
    return mpipe.writepipe(pipefd,string)
def m_read():
    return mpipe.readpipe(pipefd)
def get_online_client():
    return 0
