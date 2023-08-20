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
**App**.

Two `Edge`s, **North** and **South**, send traffic to the **West** and **East**
in different proportions.  Each edge's traffic follows a sinusoidal diurnal
pattern (with the phases of **North** and **South** offset by π/2).

The resulting `Site` that we simulate looks like this:

```
                25%
               ┌─┬─ West:  Frontend                     LBCache → Database
       North: ─┤ │                   ↘               ↗ 75%     10%
Site:          │ │                      LoadBalancer
       South: ─┼─┤                   ↗               ↘ 25%     10%
               └─┴─ East:  Frontend                     LBCache → Database
                75%
```

By default, the simulation in this example is set to simulate 12 hours of site
operation and outputs verbose execution information as well as the final state
of the site.
