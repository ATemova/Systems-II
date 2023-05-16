public class TLBSimulator {
	
	static int N = 100000;  // num. of elements of the array
	static int PAGE_SIZE = 4 * 1024;
	static int TLB_SIZE = 64;
	static int TYPE_SIZE = 4; // in bytes

	static int[] tlb;
	static int[] last_access;
	static int time, hits;

	static private void TLB_init() {
		tlb = new int[TLB_SIZE];
		last_access = new int[TLB_SIZE];
		for (int i=0; i<TLB_SIZE; i++) {
			tlb[i] = -1;
			last_access[i] = -1; // never!
		}
	}

	static boolean tlb_check(int page) {
		for (int i=0; i<TLB_SIZE; i++) {
			if (tlb[i] == page) {
				last_access[i] = time;
				return true;
			}
		}
		return false;
	}

	private static void tlb_insert(int page) {
		int old = 0;
		for (int i=1; i<TLB_SIZE; i++) {
			if (last_access[i] < last_access[old]) {
				old = i;
			}
		}
		tlb[old] = page;
		last_access[old] = time;
	} 

	private static void access(int index) {
		time++;
		int address = index * TYPE_SIZE;
		int page = address / PAGE_SIZE;
		boolean hit = tlb_check(page);
		if (!hit) {
			tlb_insert(page);
		} else {
			hits++;
		}
	}

	public static void main(String[] args) {

		TLB_init();
		time = 0;
		hits = 0;

		for (int j=1; j<=100; j++) {
			for (int i=0; i<N; i += 1024) {
				access(i);
			}
		}

		double eff = 1.0 * hits / time;
		System.out.println("TLB efficiency = " + eff);

	}

}
