Feature: Rewrites ExitTo Node Tags to Destination Way tags
# `exit_to=` is tagged on nodes in combination with the `highway=motorway_junction` tag.
# - http://wiki.openstreetmap.org/wiki/Key:exit_to
# - https://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway_junction

    Background:
        Given the profile "car"
        Given a grid size of 20 meters


    Scenario: Successful rewrite
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    | motorway_junction | ExitC   |

        And the ways
            | nodes | highway       | oneway |
            | abd   | motorway      |        |
            | bc    | motorway_link | yes    |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Left and right exits not supported, only a few hundred :left :right namespaced keys
        Given the node map
            """
                        . d .
            a . . . b `
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to:left | exit_to:right |
            | b    | motorway_junction | ExitC        | ExitD         |

        And the ways
            | nodes | highway       | oneway |
            | ab    | motorway      |        |
            | bc    | motorway_link | yes    |
            | bd    | motorway_link | yes    |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Not a motorway junction node
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    |                   | ExitC   |

        And the ways
            | nodes | highway       | oneway |
            | abd   | motorway      |        |
            | bc    | motorway_link | yes    |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Not a link road
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    | motorway_junction | ExitC   |

        And the ways
            | nodes | highway       | oneway |
            | abd   | motorway      |        |
            | bc    | motorway      | yes    |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Not a oneway link
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    | motorway_junction | ExitC   |

        And the ways
            | nodes | highway       | oneway |
            | abd   | motorway      |        |
            | bc    | motorway_link | no     |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Destination tag already present
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    | motorway_junction | ExitC   |

        And the ways
            | nodes | highway       | oneway | destination  |
            | abd   | motorway      |        |              |
            | bc    | motorway_link | yes    | DestinationC |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |

    Scenario: Destination ref tag already present
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to |
            | b    | motorway_junction | ExitC   |

        And the ways
            | nodes | highway       | oneway | destination:ref |
            | abd   | motorway      |        |                 |
            | bc    | motorway_link | yes    | DestinationC    |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |
