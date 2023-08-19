# SiteSim

SiteSim is a simple, [domain-oriented micro-service
architecture](https://www.uber.com/blog/microservice-architecture/) simulator.
You define micro-service characteristics, such as dependencies, load balancing
policies, and so on, and SiteSim simulates the flow of RPC requests through
each service in the system.

## Building

On a Unix-like system, you should be able to simply run:

```
make
```

in order to build SiteSim and run an example simulation.

## Example

The default example instantiates a simplistic architecture consisting of:

* A **Frontend** `RegionalService` that serves incoming HTTPS requests.
* An **LBCache** `LoadBalancedService` that load balances its RPC requests across
  data center `Region`s.
* A **Database** `RegionalService` that serves cache fill requests.

These services are organized into an **App** `Stack` in the following manner:

```
Frontend → LBCache → Database
```

We set up a simplistic data center infrastructure consisting of two `Regions`:
**West** and **East**.  The regions are identical in their deployments of the
**App** `Stack`.

The resulting `Site` that we simulate looks like this:

```

       West:  100RPS → Frontend                     LBCache → Database
                                 ↘               ↗ 30%
Site:                               LoadBalancer
                                 ↗               ↘ 70%
       East:  100RPS → Frontend                     LBCache → Database

```
