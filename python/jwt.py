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
import time
def key():
    return '!!JWTSecretKeyHere!!'
def claims():
    claims = {
        'iss': 'SmartIR',
        'sub': 'SmartIR',
        'exp': '!time!',
        'iat': '!time!',
        'jti': '!JWTID!',
    }
    return claims
def headers():
    headers = {
        "typ" : "JWT",
        "alg" : "HS256",
    }
    return headers
def generate_JWT():
    tmp_claims = claims()
    time = time.time()
    tmp_claims['iat']=time
    tmp_claims['exp']=time+3600 #one hour
    encoded = jwt.encode(claims, key, algorithm='HS384')
