package handlers

import "net/http"

func IndexHandler(w http.ResponseWriter, r *http.Request) {
	queryParams := r.URL.Query()
	wifi := queryParams.Get("wifi")

	templates.ExecuteTemplate(
		w,
		"index.html",
		struct{ CategoryWifi bool }{CategoryWifi: wifi != ""},
	)
}
