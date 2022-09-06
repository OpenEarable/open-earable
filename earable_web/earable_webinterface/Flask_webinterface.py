from flask import Flask, render_template

host = "localhost"
port = 5002

app = Flask(__name__, static_url_path='', static_folder='static', template_folder='templates')

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/dashboard")
def dashboard():
    print("test")
    return render_template("dashboard.html")

@app.route("/recorder")
def recorder():
    return render_template("recorder.html")

if __name__ == '__main__':
    app.run(host=host, port=port, debug=False)
