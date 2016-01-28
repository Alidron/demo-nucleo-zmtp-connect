Demo of a node connecting to another node
=========================================

[![Gitter](https://badges.gitter.im/gitterHQ/gitter.svg)](https://gitter.im/Alidron/talk)

Part of the demo run at FOSDEM talk.

This demo use a Nucleo-Spirit1 node made of eval boards. It uses a ARM Cortex-M3 at 32MHz with a sub-1GHz RF module.

The program of this demo connect to another similar node and interact with it. It uses ZMTP protocol with PUB/SUB sockets. When the user push the blue button of the Nucleo board, it toggles the blue light on the other node.

License and contribution policy
===============================

This project is licensed under LGPLv3.

Contiki-zmtp is licensed under MPLv2.

To contribute, please, follow the [C4.1](http://rfc.zeromq.org/spec:22) contribution policy.
