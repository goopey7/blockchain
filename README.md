
# blockchain
## Build on Ubuntu based distros
1. Install the OpenSSL Library

`sudo apt-get install libssl-dev`

2. Clone the project

`git clone https://github.com/goopey7/blockchain.git`

3. Enter the repository's folder

`cd blockchain-master`

4. Compile with g++

`g++ *.cpp -L/usr/lib -lssl -lcrypto  -o blockchain -pthread`

5. Run 

`./blockchain`

## Run on Windows
If you just want to run the program on Windows you can [download the build here](https://github.com/goopey7/blockchain/releases)

## Build on Windows
 1. You will need the OpenSSL library. Follow [this YouTube tutorial](https://www.youtube.com/watch?v=PMHEoBkxYaQ) on how to compile the OpenSSL library for Windows in Visual Studio 2019 or Visual Studio 2017
 2. Switch to the Windows-Compatible branch of the repository to download the source code for Windows and add it to the Visual Studio project you made in the YouTube tutorial.
 ![enter image description here](https://samcollier.tech/blockchainDocumentation/branch.png)


## Joining a Server
![Client's main menu](https://samcollier.tech/blockchainDocumentation/ClientStartMenu.png)

 1. If the main server is online, you will be able to join by entering `1`
 2. You can try to connect to a specific server by pressing `2`, you will be prompted to enter the server's ip address.
 3. You also have the ability to use this program without a server by pressing `3`.

## Using the blockchain

![enter image description here](https://samcollier.tech/blockchainDocumentation/ClientSubMenu1.png)
Once you have joined a server successfully, the program will receive a copy of the canonical blockchain stored in the server
 1. Press `1` to type in an index, and you will see the raw data stored in the block.
 2. Press `2` to delete a block. You will be prompted to enter an index of the block you'd like to delete. The program will then re-mine the blockchain to keep it valid. Note that every second, the client grabs the server's blockchain, and if the server's blockchain has more blocks than your local blockchain, your local blockchain will get overwritten by the server's, so it will cancel the effects of you deleting a block. It's best to test this through the offline mode.
 3. Access your inventory (crypto wallet) by pressing `3`. You will be prompted to paste in your private key. If you don't already have a private key, generate one by pressing `4`.
 4. Press `4` to generate a new inventory (crypto wallet). You will receive a private key. You must write it down or save it somewhere because it is the only way to access your inventory. If you loose this key, your inventory is lost.

![enter image description here](https://samcollier.tech/blockchainDocumentation/inventory.png)Once you have accessed your inventory you can do the following:

 1. Attribute an item from the game by pressing `1`. This is supposed to simulate a player receiving an item through the game that uses this blockchain. So, this event would fire after being rewarded after you complete a quest, or after picking up an item you found somewhere in the game world. You will be prompted to enter an item ID, this doesn't necessarily have to be a number, but whatever it is, it's how the game is able to determine what the item actually is. You will also be prompted to give the item a name, you can call it whatever you'd like.
 2. Make a transaction with another player by pressing `2`.  First, you will be prompted to select the item you'd wish to send to someone. The number to the left of your desired item, is the one you should enter. You will also be prompted to paste in the inventory address (public key) of the player you'd like to receive the selected item. Your inventory address is printed on the menu as seen above, so it's easy to copy and send it around.
 3. You can change the personalized name of an item by pressing `3`. The reason you may want to do this is so that you can verify to other players in a grey market situation, that you do indeed own the item they'd be willing to pay you real money for. If you can rename your item to something the buyer specifies, the buyer can then check the blockchain and verify that you do indeed own the item.

## Hosting a Server
Servers are hosted on port `9162`
|Command|Explanation|
|--|--|
| `help, h, or ?` | Displays the help screen |
|`shutdown, exit, or s`|Shutdown server|
|`difficulty set NUMBER`|Set the difficulty to specified value. Max difficulty is 10|
|`verbose or v`|Toggle output verbosity|
|`clear or cls`|Clear screen|
