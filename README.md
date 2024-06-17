# tikrtrakr

tikrtrakr is a simple cryptocurrency application that allows users to track the price of a given symbol pair on the Binance exchange. The application is made up of a daemon that fetches the latest price of a given symbol pair at a given interval, and a frontend CLI tool that parses the JSON output of the daemon and displays the price in a human-readable format.

## Table of Contents

* [Installation](#installation)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)
* [Additional Information](#additional-information)

## Installation

This project contains a custom configure/make build system. To build the project, run the following commands:

```bash
git clone https://github.com/uptudev/tikrtrakr.git
cd tikrtrakr
./configure
make
sudo make install
```

```bash
git clone https://github.com/uptudev/tikrtrakr.git
cd tikrtrakr
./configure
make
sudo make install
```

### Daemon

Start a daemon for a pair to track the price, with a default interval of 5 seconds:

```bash
tikrtrakrd <symbol_pair> [interval]
```

#### Examples

```bash
// Start a daemon for the BTCUSDT pair that fetches the price every 60 seconds
tikrtrakrd BTCUSDT 60

// Start a daemon for the XRPUSDT pair that fetches the price every 5 seconds
tikrtrakrd XRPUSDT
```

### Client

Now, you can use the frontend CLI tool to display the cached price of the pair:
```bash
tikrtrakr <symbol_pair>
```

#### Example

```bash
// Display the cached price of the BTCUSDT pair
tikrtrakr BTCUSDT
```

___

In the future, you can stop the daemon by running the following command:

```bash
tikrtrakrd stop <symbol_pair>
```
## Contributing

PRs will be reviewed and merged if they are relevant to the project. Please ensure that your PRs are well-documented and follow the project's coding standards. Thank you for contributing!

## License

This project is licensed under the MIT License. For more information, see the `LICENSE` file.
