package space.pythons.jormungandr.benchmark;

import it.unimi.dsi.webgraph.BVGraph;
import it.unimi.dsi.webgraph.NodeIterator;

public class Main {
    static String USAGE =
        "Usage: either of\n" +
        "benchmark encode [--threads <n>] <input basename> <output basename>\n" +
        "benchmark decode <input basename>";

    static void encode(String[] args) throws Throwable {
        int threads = 1;
        String in_basename = null;
        String out_basename = null;

        for (int i = 1; i < args.length; ++i) {
            if (args[i].equalsIgnoreCase("--threads")) {
                ++i;
                if (i >= args.length) {
                    System.err.println("Error: Missing argument after --threads");
                    return;
                }

                threads = Integer.parseInt(args[i], 10);
            } else if (in_basename == null) {
                in_basename = args[i];
            } else if (out_basename == null) {
                out_basename = args[i];
            } else {
                System.err.println("Error: Unknown argument '" + args[i] + "'");
                return;
            }
        }

        if (in_basename == null || out_basename == null) {
            System.err.println(USAGE);
            return;
        }
    }

    static void decode(String[] args) throws Throwable {
        if (args.length != 2) {
            System.err.println(USAGE);
            return;
        }

        var in_basename = args[1];

        long start = System.nanoTime();
        var graph = BVGraph.load(in_basename, 0);

        // Just iterate through all nodes to make the library load the graph
        var it = graph.nodeIterator();
        for (int x = 0; x < graph.numNodes(); ++x) {
            it.nextInt();
            int[] a = it.successorArray();
        }

        long stop = System.nanoTime();

        System.out.println("" + (stop - start));
    }

    public static void main(String[] args) throws Throwable {
        if (args.length < 1) {
            System.err.println(USAGE);
            return;
        }

        if (args[0].equals("encode")) {
            encode(args);
        } else if (args[0].equals("decode")) {
            decode(args);
        } else {
            System.err.println("Invalid operation '" + args[0] + "'");
        }

        // var graph = BVGraph.load(args[0], 0);
    }
}
