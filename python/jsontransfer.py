#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
from json import *

m_dict={}
def transjson(com_json):
    tmp_d=json.loads(com_json)
    if(tmp_d.has_key('a')):#has division
        if (tmp_d['a'] != 'fi'):
            m_id = tmp_d['a']
            if (m_dict.has_key(m_id)):
                bac[0]='conflicted!'
                return bac
            else:
                m_dict[m_id] = tmp_d
                bac[0]='recorded!'
                return bac
        else:#end division
            final_dict={}
            for a in m_dict:
                for key in m_dict[a]:
                    if (final_dict.has_key(key)):
                        final_dict[key] = final_dict[key] + m_dict[a][key] #combine
                    else:
                        final_dict[key] = m_dict[a][key]
            bac[0] = 'final!'
            bac[1] = final_dict
            return bac
    else:
        bac[0] = 'final!'
        bac[1] = tmp_d
        return bac
