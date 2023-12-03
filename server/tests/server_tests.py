import requests

url = "http://localhost:12345"

class TestServer:
    def test_users(self):
        username, password = "A", 123

        users = requests.get(url + "/users").json()
        assert users == []

        req_body = { "username": username, "password": password }
        assert requests.post(url + "/users", json=req_body).status_code == 200

        users = requests.get(url + "/users").json()
        assert users == [username]

        req_body = { "username": username, "password": password }
        assert requests.delete(url + "/users", json=req_body).status_code == 200

        users = requests.get(url + "/users").json()
        assert users == []

    def test_notes(self):
        username, password = "A", 123
        noteText, notesCnt = "Hello", 4

        req_body = { "username": username, "password": password }
        assert requests.post(url + "/users", json=req_body).status_code == 200
        assert requests.get(url + "/users").json() == [username]

        for i in range(1, notesCnt + 1):
            req_body = { "username": username, "password": password, "noteText": noteText * i}
            assert requests.post(url + "/notes", json=req_body).status_code == 200

        req_body = { "username": username, "password": password }
        notes = requests.get(url + "/notes", json=req_body).json()
        assert set(notes.get(username)) == {noteText * i for i in range(1, notesCnt + 1)}

        req_body = { "username": username, "password": password }
        requests.delete(url + "/users", json=req_body)
        assert requests.get(url + "/users").json() == []

    def test_session(self):
        sessions = [requests.Session() for _ in range(2)]

        for s in sessions:
            r = s.get(url + "/users")
            assert r.json() == []