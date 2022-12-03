# Server for streaming sessions on the hack lab

## Debug

```bash
pipenv run flask --debug --app server run
```

## Run

```bash
pipenv run waitress-serve server:app
```
