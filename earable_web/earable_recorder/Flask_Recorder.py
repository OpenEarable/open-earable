from flask import Flask, render_template

host = "localhost"

#host = "10.12.181.86"
#host = "10.10.10.10"
port = 5002

app = Flask(__name__, static_url_path='', static_folder='static', template_folder='templates')

@app.route("/")
def index():
    return render_template("index.html")

if __name__ == '__main__':
    app.run(host=host, port=port, debug=True)
