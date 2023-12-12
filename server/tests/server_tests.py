import requests

url = "http://localhost:12345"

class TestServer:
    def test_users(self):
        username, password = "A", 123

        users = requests.get(url + "/users").json()
        assert users == []

        req_body = { "username": username, "password": str(password) }
        assert requests.post(url + "/users", json=req_body).status_code == 200

        users = requests.get(url + "/users").json()
        assert users == [username]

        req_body = { "username": username, "password": str(password) }
        assert requests.delete(url + "/users", json=req_body).status_code == 200

        users = requests.get(url + "/users").json()
        assert users == []

    def test_notes(self):
        username, password = "A", 123
        noteText, notesCnt = "Hello", 4

        req_body = { "username": username, "password": str(password) }
        assert requests.post(url + "/users", json=req_body).status_code == 200
        req_body = { "username": username * 2, "password": str(password) }
        assert requests.post(url + "/users", json=req_body).status_code == 200

        assert set(requests.get(url + "/users").json()) == set([username, username * 2])

        for i in range(1, notesCnt + 1):
            req_body = { "username": username, "password": str(password), "noteText": noteText * i}
            assert requests.post(url + "/notes", json=req_body).status_code == 200

        req_body = { "username": username * 2, "password": str(password), "noteText": noteText}
        assert requests.post(url + "/notes", json=req_body).status_code == 200

        notes = requests.get(url + f"/notes?username={username}&password={password}").json()
        assert set(notes.get(username)) == {noteText * i for i in range(1, notesCnt + 1)}
        notes = requests.get(url + f"/notes?username={username*2}&password={password}").json()
        assert set(notes.get(username*2)) == set([noteText])

        r = requests.delete(url + f"/notes?username={username*2}&password={password}&note=0")
        assert r.status_code == 200

        notes = requests.get(url + f"/notes?username={username*2}&password={password}").json()
        assert set(notes.get(username*2)) == set()

        req_body = { "username": username, "password": str(password) }
        requests.delete(url + "/users", json=req_body)
        req_body = { "username": username * 2, "password": str(password) }
        requests.delete(url + "/users", json=req_body)

        assert requests.get(url + "/users").json() == []

    def test_validate_creds(self):
        assert requests.get(url + "/users").json() == []

        username, password = "A", 123
        req_body = { "username": username, "password": str(password) }
        assert requests.post(url + "/users", json=req_body).status_code == 200
        assert requests.get(url + "/users").json() == [username]

        assert requests.get(url + "/validate-creds?" + f"username={username}&password={password}").status_code == 200
        assert requests.get(url + "/validate-creds?" + f"username={username}&password={password + 1}").status_code == 404
