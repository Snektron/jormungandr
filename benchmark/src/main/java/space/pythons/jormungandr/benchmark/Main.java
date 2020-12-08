package space.pythons.jormungandr.benchmark;

import it.unimi.dsi.webgraph.BVGraph;
import it.unimi.dsi.webgraph.NodeIterator;
import it.unimi.dsi.webgraph.ArrayListMutableGraph;
import it.unimi.dsi.logging.ProgressLogger;

public class Main {
    static String USAGE =
        "Usage: either of\n" +
        "benchmark encode [encode options] <input basename> <output basename>\n" +
        "benchmark decode <input basename>\n" +
        "where [encode options] may consist of:\n" +
        "--threads <int>\n" +
        "--window-size <int>\n" +
        "--zeta-k <int>\n" +
        "--min-interval-size\n" +
        "--max-ref-count";

    static void encode(String[] args) throws Throwable {
        int threads = 1;
        int window_size = 7;
        int zeta_k = 3;
        int min_interval_size = 2;
        int max_ref_count = 3;

        String in_basename = null;
        String out_basename = null;

        for (int i = 1; i < args.length; ++i) {
            switch (args[i]) {
                case "--threads":
                    threads = Integer.parseInt(args[i++], 10);
                    continue;
                case "--window-size":
                    window_size = Integer.parseInt(args[i++], 10);
                    continue;
                case "--zeta-k":
                    window_size = Integer.parseInt(args[i++], 10);
                    continue;
                case "--min-interval-size":
                    window_size = Integer.parseInt(args[i++], 10);
                    continue;
                case "--max-ref-count":
                    window_size = Integer.parseInt(args[i++], 10);
                    continue;
            }

            if (in_basename == null) {
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

        var graph = new ArrayListMutableGraph(BVGraph.load(in_basename, 0));

        long start = System.nanoTime();
        BVGraph.store(
            graph.immutableView(),
            out_basename,
            window_size,
            max_ref_count,
            min_interval_size,
            zeta_k,
            BVGraph.OUTDEGREES_DELTA | BVGraph.REFERENCES_UNARY | BVGraph.RESIDUALS_ZETA,
            threads,
            new ProgressLogger()
        );
        long stop = System.nanoTime();

        System.out.println("" + (stop - start));
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
    }
}
