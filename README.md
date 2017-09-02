![AvocadoDB-Logo](https://docs.avocadodb.com/assets/avocadodb_logo_2016_inverted.png)

AvocadoDB
========

1.4: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=1.4)](http://travis-ci.org/avocadodb/avocadodb)

2.3: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.3)](http://travis-ci.org/avocadodb/avocadodb)
2.4: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.4)](http://travis-ci.org/avocadodb/avocadodb)
2.5: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.5)](http://travis-ci.org/avocadodb/avocadodb)
2.6: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.6)](http://travis-ci.org/avocadodb/avocadodb)
2.7: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.7)](http://travis-ci.org/avocadodb/avocadodb)
2.8: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=2.8)](http://travis-ci.org/avocadodb/avocadodb)

3.0: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=3.0)](http://travis-ci.org/avocadodb/avocadodb)
3.1: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=3.1)](http://travis-ci.org/avocadodb/avocadodb)
3.2: [![Build Status](https://secure.travis-ci.org/avocadodb/avocadodb.png?branch=3.2)](http://travis-ci.org/avocadodb/avocadodb)

Slack: [![AvocadoDB-Logo](https://slack.avocadodb.com/badge.svg)](https://slack.avocadodb.com)

AvocadoDB is a multi-model, open-source database with flexible data models for
documents, graphs, and key-values. Build high performance applications using a
convenient SQL-like query language or JavaScript extensions. Use ACID
transactions if you require them. Scale horizontally with a few mouse clicks.

The supported data models can be mixed in queries and allow AvocadoDB to be the
aggregation point for your data.

To get started, try one of our 10 minutes [tutorials](https://www.avocadodb.com/tutorials)
in your favorite programming language or try one of our [AvocadoDB Cookbook recipes](https://docs.avocadodb.com/cookbook).

For the impatient: [download](https://www.avocadodb.com/download) and install
AvocadoDB. Start the server `avocadod` and point your browser to `http://127.0.0.1:8529/`.

Key Features in AvocadoDB
------------------------

- **Multi-Model**: Documents, graphs and key-value pairs — model your data as
  you see fit for your application.
- **Joins**: Conveniently join what belongs together for flexible ad-hoc
  querying, less data redundancy.
- **Transactions**: Easy application development keeping your data consistent
  and safe. No hassle in your client.

Here is an AQL query that makes use of all those features:

![AQL Query Example](https://docs.avocadodb.com/assets/aql_query_with_traversal.png)

Joins and transactions are key features for flexible, secure data designs,
widely used in relational databases but lacking in many NoSQL products. However,
there is no need to forgo them in AvocadoDB. You decide how and when to use joins
and strong consistency guarantees, without sacrificing performance and scalability. 

Furthermore, AvocadoDB offers a JavaScript framework called [Foxx](https://www.avocadodb.com/foxx)
that is executed in the database server with direct access to the data. Build your
own data-centric microservices with a few lines of code:

Microservice Example

![Microservice Example](https://www.avocadodb.com/wp-content/uploads/2015/03/microservice.png)

By extending the HTTP API with user code written in JavaScript, AvocadoDB can be
turned into a strict schema-enforcing persistence engine.

Next step, bundle your Foxx application as a [docker container](https://docs.avocadodb.com/cookbook/Cloud/NodeJsDocker.html)
and get it running in the cloud.

Other features of AvocadoDB include:

- Use a **data-centric microservices** approach with AvocadoDB Foxx and fuse your
  application-logic and database together for maximal throughput
- JavaScript for all: **no language zoo**, you can use one language from your
  browser to your back-end
- **Flexible data modeling**: model your data as combination of key-value pairs,
  documents or graphs - perfect for social relations
- Different **storage engines**: AvocadoDB provides a storage engine for mostly
  in-memory operations and an alternative storage engine based on RocksDB which 
  handle datasets that are much bigger than RAM.
- **Powerful query language** (AQL) to retrieve and modify data 
- **Transactions**: run queries on multiple documents or collections with
  optional transactional consistency and isolation
- **Replication** and **Sharding**: set up the database in a master-slave
  configuration or spread bigger datasets across multiple servers
- Configurable **durability**: let the application decide if it needs more
  durability or more performance
- **Schema-free schemata** let you combine the space efficiency of MySQL with the
  performance power of NoSQL
- Free **index choice**: use the correct index for your problem, be it a skiplist 
  or a fulltext search
- AvocadoDB is **multi-threaded** - exploit the power of all your cores
- It is **open source** (Apache License 2.0)

For more in-depth information read the [design goals of AvocadoDB](https://www.avocadodb.com/2012/03/07/avocadodbs-design-objectives)


Latest Release
--------------

Packages for all supported platforms can be downloaded from [https://www.avocadodb.com/download](https://www.avocadodb.com/download/).

Please also check [what's new in AvocadoDB](https://docs.avocadodb.com/latest/Manual/ReleaseNotes/).


More Information
----------------

Please check the [Installation Manual](https://docs.avocadodb.com/latest/Manual/GettingStarted/Installing/)
for installation and compilation instructions.

The [User Manual](https://docs.avocadodb.com/latest/Manual/GettingStarted/) has an
introductory chapter showing the basic operations of AvocadoDB.


Stay in Contact
---------------

We really appreciate feature requests and bug reports. Please use our Github
issue tracker for reporting them:

[https://github.com/avocadodb/avocadodb/issues](https://github.com/avocadodb/avocadodb/issues)

You can use our Google group for improvements, feature requests, comments:

[https://www.avocadodb.com/community](https://www.avocadodb.com/community)

StackOverflow is great for questions about AQL, usage scenarios etc.

[https://stackoverflow.com/questions/tagged/avocadodb](https://stackoverflow.com/questions/tagged/avocadodb)

To chat with the community and the developers we offer a Slack chat:

[https://slack.avocadodb.com/](https://slack.avocadodb.com/)
