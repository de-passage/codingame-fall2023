# codingame-fall2023
Code for Codingame challenge Fall 2023

## Installation

This requires a referee to work. Start by cloning the challenge repository : https://github.com/CodinGame/FallChallenge2023-SeabedSecurity/tree/main
Then add a command line entry point (use this file: https://github.com/aangairbender/SpringChallenge2023-brutaltester/blob/main/src/main/java/com/codingame/gameengine/runner/CommandLineInterface.java)

Compile the project and `cp /path/to/referee/repo/target/fish*.jar /path/to/this/repo/ais/referee.jar`

Compile [cg-brutaltester](https://github.com/dreignier/cg-brutaltester) and copy the jar file to the ais folder.

## Commands

`make test` runs cg-brutaltester
`make release` builds the single file that can be synchronized with CodinGame
`make build` builds the player file
`./scripts/generate_output.bash --seed <SEED> --output output.json` generates the output of the referee as a JSON file for the game defined by the given seed.
