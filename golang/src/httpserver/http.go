package httpserver
import (
  "C"
  "net/http"
  "io"
  "log"
  "os"
)
var Logger *log.Logger
var outFile *os.File
func InitBinding(port string) string {
  //port := ":8080"
  routerBinding()
  err := http.ListenAndServe(port, nil)
  if err != nil {
    return err.Error()
  }
  return "Success."
}
func routerBinding() int {
  http.Handle("/html/", http.StripPrefix("/html/", http.FileServer(http.Dir("./html"))))
  http.HandleFunc("/cgilogin", cgi_login)
  return 0
}
func Loginit(path string) {
  if Logger == nil {
    outFile, err := os.OpenFile(path, os.O_CREATE|os.O_RDWR|os.O_APPEND, 0666)
    if err != nil {
      log.Fatalln("Failed to open log file", output, ":", err)
    }
    Logger = log.New(outFile, "", log.Ldate|log.Ltime|log.Llongfile)
    Logger.Println("Log inited")
  }
}
func CloseLogFile() {
  outFile.Close()
}
func cgi_login(rw http.ResponseWriter, req *http.Request) {
  io.WriteString(rw,"");
  req.parseForm()
}
