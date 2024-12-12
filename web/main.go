package main

import (
	"adventune/trawler-web/constants"
	"adventune/trawler-web/handlers"
	"log"
	"net/http"
)

func main() {
	// Parse all templates in the templates directory
	handlers.Init("./templates")

	// Create a file server handler
	fs := http.FileServer(http.Dir(constants.STATIC_CONTENT))

	// Strip the prefix if needed and serve files
	http.Handle("/static/", http.StripPrefix("/static/", fs))

	// Define the handlers
	http.HandleFunc("/", handlers.IndexHandler)
	http.HandleFunc("/get/aps", handlers.GetApsHandler)

	// Start the server
	log.Println("Server listening on :" + constants.PORT)
	http.ListenAndServe(":"+constants.PORT, nil)
}
