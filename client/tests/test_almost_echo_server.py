import requests

url = f"http://localhost:{1 << 13}"

class TestAlmostEchoServer:
    def test_server(self):
        body = {"hello": "world"}
        assert requests.post(url + "/save-body", json=body).status_code == 200

        r = requests.get(url + "/not-save", json={"hello!": "world!!!"})
        assert r.status_code == 200
        assert r.json() == body

        r = requests.post(url + "/any-resource", json={"hello(": "world)"})
        assert r.status_code == 200
        assert r.json() == body

        body = ["some_data", "another_data"]
        assert requests.post(url + "/save-body", json=body).status_code == 200

        r = requests.post(url, json=["some_array_element"])
        assert r.status_code == 200
        assert r.json() == body

        assert requests.post(url + "/save-status", data="400").status_code == 200
        assert requests.get(url + "/anything").status_code == 400

        assert requests.post(url + "/save-status", data="203").status_code == 200
        assert requests.get(url + "/anything").status_code == 203


