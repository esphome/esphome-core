const source = new EventSource("/events");

source.addEventListener('log', function (e) {
    const log = document.getElementById("log");
    let klass = '';
    if (e.data.startsWith("[1;31m")) {
        klass = 'e';
    } else if (e.data.startsWith("[0;33m")) {
        klass = 'w';
    } else if (e.data.startsWith("[0;32m")) {
        klass = 'i';
    } else if (e.data.startsWith("[0;35m")) {
        klass = 'c';
    } else if (e.data.startsWith("[0;36m")) {
        klass = 'd';
    } else if (e.data.startsWith("[0;37m")) {
        klass = 'v';
    } else {
        log.innerHTML += e.data + '\n';
    }
    log.innerHTML += '<span class="' + klass + '">' + e.data.substr(7, e.data.length - 10) + "</span>\n";
});

source.addEventListener('state', function (e) {
    const data = JSON.parse(e.data);
    document.getElementById(data.id).children[1].innerText = data.state;
});

const states = document.getElementById("states");
let i = 0, row;
for (; row = states.rows[i]; i++) {
    if (row.classList.contains("switch")) {
        (function(id) {
            row.children[2].children[0].addEventListener('click', function (e) {
                const xhr = new XMLHttpRequest();
                xhr.open("POST", '/switch/' + id.substr(7) + '/toggle', true);
                xhr.send();
            });
        })(row.id);
    }
    if (row.classList.contains("fan")) {
        (function(id) {
            row.children[2].children[0].addEventListener('click', function (e) {
                const xhr = new XMLHttpRequest();
                xhr.open("POST", '/fan/' + id.substr(4) + '/toggle', true);
                xhr.send();
            });
        })(row.id);
    }
    if (row.classList.contains("light")) {
        (function(id) {
            row.children[2].children[0].addEventListener('click', function (e) {
                const xhr = new XMLHttpRequest();
                xhr.open("POST", '/light/' + id.substr(6) + '/toggle', true);
                xhr.send();
            });
        })(row.id);
    }
}
