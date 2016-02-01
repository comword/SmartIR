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
import urllib2

sys.path.append(".")

import myjwt,dbman,callcpp

def initWebServer(myport):
    app = Flask(__name__,static_folder='html',template_folder='html/templates')
    app.root_path = os.getcwd()
    @app.route('/')
    def send_1():
        return redirect("dashboard.html")
    @app.route('/login_action.cgi',methods=['POST', 'GET'])
    def send_2():
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
    def send_3(first='无',second='0',third='0',fourth='0'):
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        chk_internet = internet_on()
        if chk_internet == True:
            first='良好'
        second = str(callcpp.get_online_client())
        return render_template('dashboard.html', first=first,second=second,third=third,fourth=third)
    @app.route('/templates/IRControl.html')
    def send_4():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        return render_template('IRControl.html')
    @app.route('/templates/Usermanager.html')
    def send_5():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        if (priv == 'JWTError'):
            return 'Unauthorized', 401, {'Content-Type': 'text/html'}
        return render_template('Usermanager.html')
    @app.route('/<path:filename>')
    def send_6(filename):
        return send_from_directory(app.static_folder, filename)
    @app.route('/js/<path:filename>')
    def send_7(filename):
        return send_from_directory(app.static_folder+'/js', filename)
    @app.route('/css/<path:filename>')
    def send_8(filename):
        return send_from_directory(app.static_folder+'/css', filename)
    @app.route('/fonts/<path:filename>')
    def send_9(filename):
        return send_from_directory(app.static_folder+'/fonts', filename)
    @app.route('/get_user_info.cgi',methods=['GET'])
    def send_10():
        user,priv = proc_jwt(request.cookies.get('jwt'))
        res='{"User": "'+user+'","Priv": "'+priv+'"}'
        return res,200,{'Content-Type': 'application/json'}
    @app.route('/logout.cgi',methods=['GET'])
    def send_11():
        response = make_response(redirect('login.html'))
        response.set_cookie('jwt', '')
        return response
    app.run(host="0.0.0.0",port=int(myport),threaded=True)
def internet_on():
    try:
        response=urllib2.urlopen('http://www.baidu.com',timeout=1)
        return True
    except urllib2.URLError as err: pass
    return False
def m_valid_login(username,password):
    res = dbman.verify_user(username,password)
    if (res == "Success."):
        return True
    return res
def proc_jwt(cli_jwt):
    if (cli_jwt == None):
        return 'JWTError','JWTError'
    user = myjwt.get_user(cli_jwt)
    priv = myjwt.get_privilage(cli_jwt)
    if (user == 'JWTError' or priv == 'JWTError'):
        return 'JWTError','JWTError'
    return user,priv
def startWebServer(m_pipe):
    callcpp.pipefd = m_pipe
    if (dbman.check_dbs() != True):
        dbman.create_NewDB()
    initWebServer("5000")
if __name__ == "__main__":
    if (dbman.check_dbs() != True):
        dbman.create_NewDB()
    initWebServer("5000")
