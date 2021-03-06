#!/bin/bash

# Desc: Format CSV to GPX
# Author: Petr Posvic <petr@posvic.cz>

# Example content (dateTtime altitude latitude longitude satellities):
# 2016-12-23T05:26:23.0Z 475.70 50.16357 14.88286 4
# Create workout file from datalog: cp DATALOG.TXT workout.txt
FILE="workout.txt"

if [ ! -f $FILE ]; then
  >&2 echo "Can't find file $FILE"
  exit 1
fi

function parse {
  echo "$1" | cut -d' ' -f$2 | tr -d '\r\n'
}

# Head
echo '<?xml version="1.0" encoding="UTF-8"?>
<gpx
  version="1.1"
  creator="Runkeeper - http://www.runkeeper.com"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xmlns="http://www.topografix.com/GPX/1/1"
  xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd"
  xmlns:gpxtpx="http://www.garmin.com/xmlschemas/TrackPointExtension/v1">'

timestamp=`head $FILE -n1 | cut -d' ' -f1`
date=`echo $timestamp | cut -d'T' -f1`
time=`echo $timestamp | cut -d'T' -f2`
month=`echo $date | cut -d'-' -f2`
day=`echo $date | cut -d'-' -f3`
year=`echo $date | cut -d'-' -f1`
hour=`echo $time | cut -d':' -f1`
min=`echo $time | cut -d':' -f2`
daytime="am"
if [[ ${hour:0:1} == "0" ]]; then
  if [ ${#hour} -eq 2 ]; then
    hour=${hour:1}
  fi
fi

if [ $((hour)) -gt 11 ]; then
  hour=$((hour - 12))
  daytime="pm"
fi

echo "<trk>
  <name><![CDATA[Walking $month/$day/$year $hour:$min $daytime]]></name>
  <time>$timestamp</time>
<trkseg>"

# Body
while read line; do
  echo -n "<trkpt lat=\""
  parse "$line" 3
  echo -n "\" lon=\""
  parse "$line" 4
  echo -n "\"><ele>"
  parse "$line" 2
  echo -n "</ele><time>"
  parse "$line" 1
  echo "</time></trkpt>"
done < $FILE

# Foot
echo '</trkseg>
</trk>
</gpx>'

