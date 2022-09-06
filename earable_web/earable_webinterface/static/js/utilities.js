// Add value and sift if too long
Object.defineProperty(Array.prototype, 'fixedAdd', {
    value: function(value, max) {
        this.push(value);
        if (this.length > max) {
            this.shift();
        }
    }
});

export function isWebBluetoothEnabled() {
    if (!navigator.bluetooth) {
        console.log('Web Bluetooth is NOT available!')
        return false
    }
    console.log('Web Bluetooth is available!')
    return true
}

export function floatToBytes(value) {
    var tempArray = new Float32Array(1);
    tempArray[0] = value;
    return new Uint8Array(tempArray.buffer);
}

export function intToBytes(value) {
    var tempArray = new Int32Array(1);
    tempArray[0] = value;
    return new Uint8Array(tempArray.buffer);
}

export function exportToCsv(filename, rows) {
    var processRow = function (row) {
        var finalVal = '';
        for (var j = 0; j < row.length; j++) {
            var innerValue = row[j] === null ? '' : row[j].toString();
            if (row[j] instanceof Date) {
                innerValue = row[j].toLocaleString();
            };
            var result = innerValue.replace(/"/g, '""');
            if (result.search(/("|,|\n)/g) >= 0)
                result = '"' + result + '"';
            if (j > 0)
                finalVal += ',';
            finalVal += result;
        }
        return finalVal + '\n';
    };

    var csvFile = '';
    for (var i = 0; i < rows.length; i++) {
        csvFile += processRow(rows[i]);
    }

    var blob = new Blob([csvFile], { type: 'text/csv;charset=utf-8;' });
    if (navigator.msSaveBlob) { // IE 10+
        navigator.msSaveBlob(blob, filename);
    } else {
        var link = document.createElement("a");
        if (link.download !== undefined) { // feature detection
            // Browsers that support HTML5 download attribute
            var url = URL.createObjectURL(blob);
            link.setAttribute("href", url);
            link.setAttribute("download", filename);
            link.style.visibility = 'hidden';
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }
    }
}

export function swap_html_class(elem, orig, replace) {
    if (elem.classList.contains(orig)) {
        elem.classList.remove(orig);
        elem.classList.add(replace);
    }
}

export function remove_html_class(elem, name) {
    if (elem.classList.contains(name)) {
        elem.classList.remove(name);
    }
}

export function add_html_class(elem, name) {
    if (!(elem.classList.contains(name))) {
        elem.classList.add(name);
    }
}

export class Timer {
    constructor(timeout) {
        this.timeout = timeout;
        this.compare = new Date();
    }

    set() {
        this.compare = new Date();
    }

    check() {
        let new_date, delta;
        new_date = new Date();
        delta = (new_date.getTime() - this.compare.getTime());
        if (delta > this.timeout) {
            this.compare = new_date;
            return true;
        }
        return false;
    }
}