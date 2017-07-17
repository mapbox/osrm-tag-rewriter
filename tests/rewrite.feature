Feature: Tag Rewriter Tests

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
            | node | highway           | exit_to | ref |
            | b    | motorway_junction | A       | 2   |

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
            | node | highway           | exit_to:left | exit_to:right | ref:left | ref:right |
            | b    | motorway_junction | A            | B             |        2 |         3 |

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
            | node | highway | exit_to | ref |
            | b    |         | A       |   2 |

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
            | node | highway           | exit_to | ref |
            | b    | motorway_junction |       A |   2 |

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
            | node | highway           | exit_to | ref |
            | b    | motorway_junction |      A  |   2 |

        And the ways
            | nodes | highway       | oneway |
            | abd   | motorway      |        |
            | bc    | motorway_link | no     |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Tag already present
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to | ref |
            | b    | motorway_junction |       A |   2 |

        And the ways
            | nodes | highway       | oneway | destination  | junction:ref |
            | abd   | motorway      |        |              |              |
            | bc    | motorway_link | yes    |            B |            2 |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |


    Scenario: Ref tag already present
        Given the node map
            """
            a . . . b . . d .
                      ` . c .
            """

       And the nodes
            | node | highway           | exit_to | ref |
            | b    | motorway_junction |       A |   2 |

        And the ways
            | nodes | highway       | oneway | destination:ref | junction:ref |
            | abd   | motorway      |        |                 |              |
            | bc    | motorway_link | yes    |               B |            3 |

       When I route I should get
            | waypoints | route    |
            | a,c       | ab,bc,bc |

