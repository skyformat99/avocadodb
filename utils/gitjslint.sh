#!/bin/bash

if test "`git status --short | grep '^\(.[MAU]\|[MAU].\) .*js$' | wc -l`" -eq 0; then
  exit 0;
fi

if [ -z "${ARANGOSH}" ];  then
  if [ -x build/bin/avocadosh ];  then
    ARANGOSH=build/bin/avocadosh
  elif [ -x bin/avocadosh ];  then
    ARANGOSH=bin/avocadosh
  else
    echo "$0: cannot locate avocadosh"
  fi
fi

for file in ` git status --short | grep '^\(.[MAU]\|[MAU].\) .*js$' | cut -d " " -f 3`; do
  ${ARANGOSH} -c etc/relative/avocadosh.conf --log.level error --jslint $file || exit 1
done
