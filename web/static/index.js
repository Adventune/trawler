const AttackStateEnum = {
    IDLE: 0,
    READY: 1,
    RUNNING: 2,
    FINISHED: 3,
};
const AttackTypeEnum = {
    ATTACK_TYPE_PASSIVE: 0,
    ATTACK_TYPE_DEAUTH: 1,
    ATTACK_TYPE_DOS: 2,
};
const AttackCategoryEnum = { WIFI: 0 };

let selectedApElement = -1;
let poll;
const poll_interval = 1000;
const server_ip = 'http://192.168.1.102';

function selectAp(el) {
    console.log(el.id);
    if (selectedApElement !== -1) {
        selectedApElement.classList.remove('selected');
    }
    selectedApElement = el;
    el.classList.add('selected');
}

// Prepares the attack on the server
function prepareAttack() {
    if (selectedApElement === -1) {
        console.log('No AP selected. Attack not started.');
        return;
    }

    const arrayBuffer = new ArrayBuffer(4);
    let uint8Array = new Uint8Array(arrayBuffer);
    uint8Array[0] = parseInt(selectedApElement.id);
    uint8Array[1] = parseInt(document.getElementById('attack_type').value);
    uint8Array[2] = parseInt(document.getElementById('attack_timeout').value);
    let oReq = new XMLHttpRequest();
    oReq.open('POST', server_ip + '/wifi-attack-prepare', true);
    oReq.send(arrayBuffer);

    setTimeout(getStatus, 1000);
}

function updateConfigurableFields(el) {
    switch (parseInt(el.value)) {
        case AttackTypeEnum.ATTACK_TYPE_PASSIVE:
            console.log('PASSIVE configuration');
            break;
        case AttackTypeEnum.ATTACK_TYPE_DEAUTH:
            console.log('HANDSHAKE configuration');
            document.getElementById('attack_timeout').value = 60;
            break;
        case AttackTypeEnum.ATTACK_TYPE_DOS:
            console.log('DOS configuration');
            document.getElementById('attack_timeout').value = 120;
            break;
        default:
            console.log('Unknown attack type');
            break;
    }
}

function resetAttack() {
    let oReq = new XMLHttpRequest();
    oReq.open('GET', server_ip + '/attack-reset', true);
    oReq.send();

    setTimeout(getStatus, 1000);
}

function getStatus() {
    let oReq = new XMLHttpRequest();
    oReq.onload = function() {
        let arrayBuffer = oReq.response;
        if (arrayBuffer) {
            const attack_state = parseInt(
                new Uint8Array(arrayBuffer, 0, 1).toString()
            );
            const attack_category = parseInt(
                new Uint8Array(arrayBuffer, 1, 1).toString()
            );
            const attack_content = new Uint8Array(arrayBuffer, 2);
            const attack_content_size = attack_content.byteLength;

            const attack_category_text = getCategoryText(attack_category);

            console.log(
                'attack_state=' +
                attack_state +
                '; attack_category=' +
                attack_category +
                '; attack_content_size=' +
                attack_content_size
            );

            // Clear status
            document.getElementById('attack_status').innerHTML = '';
            document.getElementById('attack_content').innerHTML = '';

            switch (attack_state) {
                case AttackStateEnum.IDLE:
                    showIdle();
                    break;
                case AttackStateEnum.READY:
                    showAttackConfig(attack_category_text);
                    break;
                case AttackStateEnum.RUNNING:
                    showRunning(
                        attack_category_text,
                        attack_content,
                        attack_content_size
                    );
                    console.log('Poll');
                    setTimeout(getStatus, poll_interval);
                    break;
                case AttackStateEnum.FINISHED:
                    showResult(
                        attack_category_text,
                        attack_content,
                        attack_content_size
                    );
                    break;
                default:
                    showIdle();
                    break;
            }
        }
    };
    oReq.onerror = function() {
        console.log('Request error');
        document.getElementById('errors').innerHTML =
            'Cannot reach ESP32. Check that you are connected to management AP. You might get disconnected during attack.';
        getStatus();
    };
    oReq.ontimeout = function() {
        console.log('Request timeout');
        getStatus();
    };
    oReq.open('GET', server_ip + '/attack-content', true);
    oReq.responseType = 'arraybuffer';
    oReq.send();
}

// Start attack
function startAttack() {
    // Get reguest to /attack-start
    let oReq = new XMLHttpRequest();
    oReq.open('GET', server_ip + '/attack-start', true);
    oReq.send();

    // start polling
    setTimeout(getStatus, poll_interval);
}

// Stop attack
function stopAttack() {
    // Get /attack-stop
    let oReq = new XMLHttpRequest();
    oReq.open('GET', server_ip + '/attack-stop', true);
    oReq.send();
    setTimeout(getStatus, poll_interval);
}

function showIdle() {
    document.getElementById('attack_status').innerHTML = 'Status: IDLE';
    document.getElementById('attack_content').innerHTML = '';
}

function showAttackConfig(category) {
    document.getElementById('attack_status').innerHTML =
        'Status: READY, Category: ' + category;
    document.getElementById('attack_content').innerHTML = '';
}

function showRunning(category, content, contentSize) {
    document.getElementById('attack_status').innerHTML =
        'Status: RUNNING, Category: ' + category;

    document.getElementById('attack_content').innerHTML = '';
    let handshakes = '';
    for (let i = 0; i < contentSize; i = i + 1) {
        handshakes += uint8ToHex(content[i]);
        if (i % 50 === 49) {
            handshakes += '\n';
        }
    }
    document.getElementById('attack_content').innerHTML +=
        '<pre><code>' + handshakes + '</code></pre>';
}

function showResult(category, content, contentSize) {
    document.getElementById('attack_status').innerHTML =
        'Status: FINISHED, Category: ' + category;

    document.getElementById('attack_content').innerHTML = '';
    let pcap_link = document.createElement('a');
    pcap_link.setAttribute('href', 'capture.pcap');
    pcap_link.text = 'Download PCAP file';
    let hccapx_link = document.createElement('a');
    hccapx_link.setAttribute('href', 'capture.hccapx');
    hccapx_link.text = 'Download HCCAPX file';
    document.getElementById('attack_content').innerHTML +=
        '<p>' + pcap_link.outerHTML + '</p>';
    document.getElementById('attack_content').innerHTML +=
        '<p>' + hccapx_link.outerHTML + '</p>';

    let handshakes = '';
    for (let i = 0; i < contentSize; i = i + 1) {
        handshakes += uint8ToHex(content[i]);
        if (i % 50 === 49) {
            handshakes += '\n';
        }
    }
    document.getElementById('attack_content').innerHTML +=
        '<pre><code>' + handshakes + '</code></pre>';
}

// UTILS

/**
 * Attack category int to text
 */
function getCategoryText(category_int) {
    switch (category_int) {
        case AttackCategoryEnum.WIFI:
            return 'WIFI';
        default:
            return 'UNKNOWN';
    }
}

/**
 * Uint8 number to hex
 */
function uint8ToHex(uint8) {
    return ('00' + uint8.toString()).slice(-2);
}
