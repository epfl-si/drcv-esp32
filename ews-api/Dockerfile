FROM node:18-alpine

WORKDIR /app

COPY ews-api/package.json ./
COPY ews-api/.env /app/.env
COPY .env .env

RUN npm install

COPY ews-api/server.js .

EXPOSE 5000

CMD ["npm", "start"]
