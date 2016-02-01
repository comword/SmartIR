#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
from jose import jwt
from jose.exceptions import JWTError
from datetime import datetime
from datetime import timedelta

def key():
    return '!!JWTSecretKeyHere!!'
def claims():
    claims = {
        'iss': 'SmartIR',
        'sub': 'SmartIR',
        'exp': '!time!',
        'iat': '!time!',
        'user': '!user!',
        'privilage': '!privilage!'
#        'jti': '!JWTID!',
    }
    return claims
def headers():
    headers = {
        "typ" : "JWT",
        "alg" : "HS256",
    }
    return headers
def generate_JWT(User,Privilage):
    tmp_claims = claims()
    time = datetime.utcnow()
    tmp_claims['iat']=time
    tmp_claims['exp']=time+timedelta(hours=1) #one hour
    tmp_claims['user']=User
    tmp_claims['privilage']=Privilage
    encoded = jwt.encode(tmp_claims, key(), algorithm='HS384')
    return encoded
def renew_JWT(m_jwt):
    try:
        decode = jwt.decode(m_jwt, key, algorithms='HS384')
    except JWTError as err:
        return 'Error'
    if decode['exp'] < datetime.utcnow():# expired
        return 'Error'
    if decode['exp']-timedelta(minutes=20) < datetime.utcnow():
        return generate_JWT(decode['user'],decode['privilage'])
    return 'NotNess' #not necessary
def verify_JWT(m_jwt):
    try:
        decode = jwt.decode(m_jwt, key(), algorithms='HS384')
    except JWTError as err:
        return False
    return True
def get_user(m_jwt):
    try:
        decode = jwt.decode(m_jwt, key(), algorithms='HS384')
    except JWTError as err:
        return "JWTError"
    return decode['user']
def get_privilage(m_jwt):
    try:
        decode = jwt.decode(m_jwt, key(), algorithms='HS384')
    except JWTError as err:
        return "JWTError"
    return decode['privilage']
