# Rives Asteroids
Fun personal project to test the [RIV](https://docs.rives.io/docs/riv/introduction) console.

>RIV is a low-level verifiable fantasy console for building small games that runs on a special RISC-V Linux machine.

# Installing and running (Ubuntu)
## Install RIVES
For more details or other platforms go to [RIVES Documentation](https://docs.rives.io/docs/riv/introduction](https://docs.rives.io/docs/riv/getting-started))
```
wget -O rivemu https://github.com/rives-io/riv/releases/latest/download/rivemu-linux-amd64
chmod +x rivemu
```
```
mkdir -p $HOME/.riv
mv rivemu $HOME/.riv/
echo 'export PATH=$HOME/.riv:$PATH' >> $HOME/.bashrc
export PATH=$HOME/.riv:$PATH
```
## Run the game
```
rivemu -workspace -exec riv-jit-c asteroids.c
```
## Play the game
<kbd>&#8593;</kbd> Thrust

<kbd>&#8592;</kbd><kbd>&#8594;</kbd> Turn

<kbd>Z</kbd> Shoot
