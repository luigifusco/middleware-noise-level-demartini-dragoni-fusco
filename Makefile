
up:
	docker compose up --build -d

down:
	docker compose down

attach:
	docker compose logs -f

clear:
	docker compose rm -v -s -f

reset:
	docker compose rm -v -s
	docker compose up --build -d
