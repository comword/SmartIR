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
    encoded = jwt.encode(claims, key, algorithm='HS384')
    return encoded
def renew_JWT(old_claims):
    try:
        decode = jwt.decode(old_claims, key, algorithms='HS384')
    except JWTError as err:
        return "JWTError"
    if decode['exp'] < datetime.utcnow():# expired
        return "JWTexpired"
    return generate_JWT(decode['user'],decode['privilage'])
def verify_JWT(jwt):
    try:
        decode = jwt.decode(jwt, key, algorithms='HS384')
    except JWTError as err:
        return False
    return True
def get_user(jwt):
    try:
        decode = jwt.decode(jwt, key, algorithms='HS384')
    except JWTError as err:
        return "JWTError"
    return decode['user']
def get_privilage(jwt):
    try:
        decode = jwt.decode(jwt, key, algorithms='HS384')
    except JWTError as err:
        return "JWTError"
    return decode['privilage']
