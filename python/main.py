#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import os,sys
from flask import Flask, request, send_from_directory, render_template, redirect, make_response
import urllib, logging, logging.config
from json import *

sys.path.append(".")

import myjwt,dbman,callcpp

global datadir
datadir = ""

def initWebServer(myport):
    app = Flask(__name__,static_folder='html',template_folder='html/templates')
    app.root_path = os.getcwd()
    logging.config.fileConfig(app.root_path+'/python/'+'logging.conf')
    logger = logging.getLogger('simple')
    @app.route('/')
    def send_1():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return redirect("login.html")
        return redirect("dashboard.html")
    @app.route('/login.html')
    def send_2():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return send_from_directory(app.static_folder, 'login.html')
        if (priv == 'JWSError'):
            response = make_response('login.html')
            response.set_cookie('jwt', '')
            return response
        return redirect("dashboard.html")
    @app.route('/login_action.cgi',methods=['POST', 'GET'])
    def send_3():
        error = None
        if request.method == 'POST':
            user = request.form['inputUsername']
            if (m_valid_login(user,request.form['inputPassword']) == True):
                privilage = dbman.get_privilage(user);
                client_jwt = myjwt.generate_JWT(user,privilage)
                response = make_response('dashboard.html')
                response.set_cookie('jwt', client_jwt)
                return response
            return "Wrong username or wrong password."
        return 'Bad Request', 400, {'Content-Type': 'text/html'}#Generate http 400
    @app.route('/templates/dashboard.html')
    def send_4(first='无',second='0',third='0',fourth='0'):
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        chk_internet = internet_on()
        if chk_internet == True:
            first='良好'
        second = str(callcpp.get_online_client())
        fourth = str(dbman.get_user_count())
        return render_template('dashboard.html', first=first,second=second,third=third,fourth=fourth)
    @app.route('/templates/IRControl.html')
    def send_5():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        return render_template('IRControl.html')
    @app.route('/templates/Usermanager.html')
    def send_6():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        return render_template('Usermanager.html')
    @app.route('/<path:filename>')
    def send_7(filename):
        return send_from_directory(app.static_folder, filename)
    @app.route('/js/<path:filename>')
    def send_8(filename):
        return send_from_directory(app.static_folder+'/js', filename)
    @app.route('/css/<path:filename>')
    def send_9(filename):
        return send_from_directory(app.static_folder+'/css', filename)
    @app.route('/fonts/<path:filename>')
    def send_10(filename):
        return send_from_directory(app.static_folder+'/fonts', filename)
    @app.route('/get_user_info.cgi',methods=['GET'])
    def send_11():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        res='{"User": "'+user+'","Priv": "'+priv+'"}'
        return res,200,{'Content-Type': 'application/json'}
    @app.route('/logout.cgi',methods=['GET'])
    def send_12():
        response = make_response(redirect('login.html'))
        response.set_cookie('jwt', '')
        return response
    @app.route('/renew_jwt.cgi',methods=['GET'])
    def send_13():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        response = make_response()
        renew = myjwt.renew_JWT(request.cookies.get('jwt'))
        if (renew == 'Error'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        if (renew == 'NotNess'):
            return ''
        else:
            response.set_cookie('jwt', renew)
            return response
    @app.route('/study_IR.cgi',methods=['POST', 'GET'])
    def send_14():
        if request.method == 'POST':
            user,priv = proc_jwt(request.cookies.get('jwt'))
            if (priv == 'JWTError'):
                return 'Unauthorized', 401, {'Content-Type': 'text/html'}
            IRID = request.form['IRID']
            m_dict={}
            m_dict["b"]="b"
            m_dict["c"]=int(IRID)
            callcpp.m_write_dict(m_dict)
        return 'Bad Request', 400, {'Content-Type': 'text/html'}
    @app.route('/get_IR_recode.cgi')
    def send_15():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        st = request.args['rangeL']
        num = request.args['num']
        res = dbman.get_IR_dict(st,num)
        response = make_response(JSONEncoder().encode(res))
        return respond
    @app.route('/go_IR_action.cgi',methods=['POST', 'GET'])
    def send_16():
        if request.method == 'POST':
            user,priv = proc_jwt(request.cookies.get('jwt'))
            if (priv == 'JWTError'):
                return 'Unauthorized', 401, {'Content-Type': 'text/html'}
            action = request.form['action']
            operator = {'send':IR_action_Send,'modify':IR_action_Modify,'remove':IR_action_Remove}
            res = operator.get(action)(request.form['m_data'])
            respond = make_response(res)
            return respond
        return 'Bad Request', 400, {'Content-Type': 'text/html'}
    @app.route('/get_user_list.cgi')
    def send_17():
        #{ID:username}
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        num = request.args['num']
        st = request.args['start']
        res = dbman.get_user_list(st,num)
        return JSONEncoder().encode(res),200,{'Content-Type': 'application/json'}
    @app.route('/get_IR_learn_proc.cgi')
    def send_18():
        res = callcpp.m_read()
        return ''
    @app.route('/get_priv_list.cgi')
    def send_19():
        #{username:privilage}
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        num = request.args['num']
        st = request.args['start']
        res = dbman.get_priv_list(st,num)
        return JSONEncoder().encode(res),200,{'Content-Type': 'application/json'}
    @app.route('/modify_password.cgi',methods=['POST', 'GET'])
    def send_20():
        if request.method == 'POST':
            user,priv = proc_jwt(request.cookies.get('jwt'))
            if (priv == 'JWTError'):
                return 'Unauthorized', 401, {'Content-Type': 'text/html'}
            u_name = request.form['username']
            oripass = request.form['oripass']
            newpass = request.form['newpass']
            if ((user != u_name) and (priv != "admin")):
                return 'Privilage Error', 400, {'Content-Type': 'text/html'}
            if (dbman.verify_user(u_name,oripass) != "Success."):
                return 'Original password wrong', 400, {'Content-Type': 'text/html'}
            if (dbman.change_user_pass(u_name,newpass) == "Success."):
                respond = make_response("Success.")
                if (u_name == user): #revoke jwt
                    respond = make_response('Relogin.')
                    respond.set_cookie('jwt', '')
                return respond
        return 'Bad Request', 400, {'Content-Type': 'text/html'}
    @app.route('/get_operation_log.cgi')
    def send_21():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        respond = make_response("Success.")
        return respond
    @app.route('/get_ready_info.cgi')
    def send_22():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        respond = make_response("Success.")
        return respond
    app.run(host="0.0.0.0",port=int(myport),threaded=True)
def internet_on():
    try:
        response = urllib2.urlopen('http://www.baidu.com',timeout=1)
        return True
    except urllib2.URLError as err: pass
    return False
def m_valid_login(username,password):
    res = dbman.verify_user(username,password)
    if (res == "Success."):
        return True
    return res
def proc_jwt(cli_jwt):
    if (cli_jwt == None or cli_jwt == ''):
        return 'JWTError','JWTError'
    user = myjwt.get_user(cli_jwt)
    priv = myjwt.get_privilage(cli_jwt)
    if (user == 'JWTError' or priv == 'JWTError'):
        return 'JWTError','JWTError'
    if (user == 'JWSError' or priv == 'JWSError'):
        return 'JWSError','JWSError'
    return user,priv
def start_IR_learn(IRID):
    m_dic={}
    m_dic['b']="b"
    m_dic["c"]=IRID
    callcpp.m_write_dict(m_dic)
    return True
def IR_action_Send(data):
    m_dic={}
    m_dic['b']="a"
    m_dic["c"]=data["IRID"]
    m_dic["d"]=data["IRdata"]
    callcpp.m_write_dict(m_dic)
    return True
def IR_action_Modify(data):
    return True
def IR_action_Remove(data):
    return True
def startWebServer(m_pipe_rpy,m_pipe_tpy,datadirectory):
    global datadir
    datadir = datadirectory
    callcpp.pipe_tfd = m_pipe_rpy
    callcpp.pipe_rfd = m_pipe_tpy
    dbman.check_dbs()
    if (dbman.get_user("admin")==-1):
        dbman.reset_admin_user()
    initWebServer("5000")
def rd_database(dbname,key):
    return dbman.read_database(dbname,key)
def we_database(dbname,key,value):
    return dbman.write_database(dbname,key,value)
def we_IR_detail(IRID,value):
    res = dbman.write_IR_detail(IRID,value)
    if(res == "IRID not exist."):
        return 1
    elif(res == "Success."):
        return 0
if __name__ == "__main__":
    dbman.check_dbs()
    initWebServer("5000")
