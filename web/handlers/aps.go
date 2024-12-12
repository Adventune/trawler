package handlers

import (
	"adventune/trawler-web/constants"
	"bytes"
	"encoding/hex"
	"fmt"
	"io"
	"net/http"
)

type APList struct {
	SSID  string
	BSSID string
	RSSI  int
	ID    int
}

// fetchAPList fetches the AP list from the external API returning `application/octet-stream`.
func fetchAPList(apiURL string) ([]byte, error) {
	resp, err := http.Get(apiURL)
	if err != nil {
		return nil, fmt.Errorf("failed to fetch AP list: %v", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("unexpected status code: %d", resp.StatusCode)
	}

	if resp.Header.Get("Content-Type") != "application/octet-stream" {
		return nil, fmt.Errorf("unexpected content type: %s", resp.Header.Get("Content-Type"))
	}

	return io.ReadAll(resp.Body)
}

func GetApsHandler(w http.ResponseWriter, r *http.Request) {
	apiURL := constants.SERVER + "/ap-list" // Replace with the actual API endpoint.

	// Fetch AP list from the external API.
	apListRes, err := fetchAPList(apiURL)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	var apList []APList
	apIndex := 0

	// Process the fetched AP data.
	for i := 0; i < len(apListRes); i += 40 {
		if i+40 > len(apListRes) {
			break // Avoid out-of-bounds in case of incomplete data.
		}
		ssid := string(bytes.Trim(apListRes[i:i+32], "\x00"))
		bssid := apListRes[i+32 : i+38]
		rssi := int(apListRes[i+39]) - 255

		// Format BSSID as a colon-separated string.
		var bssidStr string
		for j, b := range bssid {
			bssidStr += hex.EncodeToString([]byte{b})
			if j < len(bssid)-1 {
				bssidStr += ":"
			}
		}

		apList = append(apList, APList{SSID: ssid, BSSID: bssidStr, RSSI: rssi, ID: apIndex})
		apIndex += 1

	}
	templates.ExecuteTemplate(w, "ap_table", apList)
}
