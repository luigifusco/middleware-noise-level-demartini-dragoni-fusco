
up:
	docker compose up --build -d

down:
	docker compose down

attach:
	docker compose logs -f

clear:
	docker compose rm -v -s -f

prune:
	docker volume prune

reset:
	docker compose rm -v -s -f
	docker compose up --build -d

