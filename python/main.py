#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
import os,sys
from flask import Flask, request, send_from_directory, render_template
import urllib2
sys.path.append(".")

import jwt,dbman

def initWebServer(myport):
    app = Flask(__name__,static_folder='html',template_folder='html/templates')
    app.root_path = os.getcwd()
    @app.route('/login_action.cgi',methods=['POST', 'GET'])
    def send_6():
        error = None
        if request.method == 'POST':
            if m_valid_login(request.form['inputUsername'],request.form['inputPassword']):
                return 'Success!'
            return "Wrong username or wrong password."
        tmp = request.form['Dummy'] #Generate http 400
    @app.route('/templates/dashboard.html')
    def send_5(first='无',second='0',third='0',fourth='0'):
        chk_internet = internet_on()
        if chk_internet == True:
            first='良好'
        return render_template('dashboard.html', first=first,second=second,third=third,fourth=third)
    @app.route('/<path:filename>')
    def send_1(filename):
        return send_from_directory(app.static_folder, filename)
    @app.route('/js/<path:filename>')
    def send_2(filename):
        return send_from_directory(app.static_folder+'/js', filename)
    @app.route('/css/<path:filename>')
    def send_3(filename):
        return send_from_directory(app.static_folder+'/css', filename)
    @app.route('/fonts/<path:filename>')
    def send_4(filename):
        return send_from_directory(app.static_folder+'/fonts', filename)
    app.run(host="0.0.0.0",port=int(myport), threaded=True)
def internet_on():
    try:
        response=urllib2.urlopen('http://www.baidu.com',timeout=1)
        return True
    except urllib2.URLError as err: pass
    return False
def get_online_client():
    return 0
def m_valid_login(username,password):
    return True
def startWebServer():
    initWebServer("5000")
if __name__ == "__main__":
    initWebServer("5000")
