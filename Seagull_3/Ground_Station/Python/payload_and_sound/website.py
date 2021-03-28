import json
import pickle
import random
import string
from flask import Flask, render_template


app = Flask(__name__)
app.secret_key = ''.join(random.choice(string.printable) for i in range(1023))
app.config['MAX_CONTENT_LENGTH'] = 1000 * 1024 * 1024 # 1 GB file max


@app.route(r'/')
def home_form():
    return render_template(r'indicators.html')

@app.route(r'/indicatorData')
def indicator_data():
    fp = open('shared.pkl', 'rb')
    return json.dumps(pickle.load(fp))


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=False, use_reloader=False, threaded=True)