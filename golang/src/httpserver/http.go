package httpserver
import (
  "C"
  "net/http"
)
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
  return 0
}
