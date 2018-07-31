FROM python:3.6.4-alpine3.7
MAINTAINER Filipe Oliveira "filipe.oliveira@im2model.com"
EXPOSE 5000

RUN echo "http://dl-8.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories

RUN apk --no-cache --update-cache add gcc gfortran python python-dev py-pip build-base wget freetype-dev libpng-dev openblas-dev

RUN ln -s /usr/include/locale.h /usr/include/xlocale.h

RUN apk update && apk upgrade && apk add bash && apk add freetype && apk add libpng && apk add curl && apk add --no-cache supervisor && apk add tzdata

RUN cp /usr/share/zoneinfo/Europe/London /etc/localtime && echo "Europe/London" >  /etc/timezone

RUN mkdir -p /etc/supervisor.d

# Supervisor config
COPY  supervisord.conf /etc/supervisord.conf

COPY . /app
WORKDIR /app
RUN pip install -r requirements.txt

CMD ["supervisord", "-c", "/etc/supervisord.conf" ]
