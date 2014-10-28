#!/bin/sh
#
# Simple script for running the server. The server is run in the background like
# a daemon process.
#
# Author: Michael Morckos <mmorckos@uwaterloo.ca>
#

## Change dir ##
cd ../

## Start icebox ##
echo "Starting icebox..."
nohup icebox --Ice.Config=./config/icebox.cfg &
sleep 1

## Start entity server ##
echo "Starting entity server..."
nohup ./bin/entityserver --Ice.Config=./config/entityserver.cfg &
sleep 1

## Start frontend server ##
echo "Starting frontend server..."
nohup ./bin/frontendserver --Ice.Config=./config/frontendserver.cfg &
sleep 1

## Start Glacier2 router instance for entity server ##
echo "Starting Glacier2 router for entity server..."
nohup glacier2router --Ice.Config=./config/glacier2router_entity.cfg &
sleep 1

## Start Glacier2 router instance for frontend server ##
echo "Starting Glacier2 router for frontend server..."
nohup glacier2router --Ice.Config=./config/glacier2router_frontend.cfg &
sleep 1

echo "Success."
