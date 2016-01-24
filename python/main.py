# Copyright 2016 Henorvell Ge
#
# This file is a part of SmartIR
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
from flask import Flask, request, send_from_directory
import urllib2
def initWebServer(myport):
    app = Flask(__name__,static_folder='../html')
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
    app.run(host="0.0.0.0",port=int(myport),threaded=True)
def internet_on():
    try:
        response=urllib2.urlopen('http://www.baidu.com',timeout=1)
        return True
    except urllib2.URLError as err: pass
    return False
def startWebServer():
    initWebServer("5000")
if __name__ == "__main__":
    initWebServer("5000")
