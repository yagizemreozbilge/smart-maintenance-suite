import { useState, useEffect, useMemo, useCallback } from 'react';
import { useMaintenance } from './MaintenanceContext.js';

// Custom Hook: useMachineStats
// Demonstrates Point 4 (useMemo) and Point 5 (Custom Hooks)
export function useMachineStats() {
    const { machines } = useMaintenance();

    // useMemo: Only recalculate stats if the machines array changes
    const stats = useMemo(() => {
        const machineList = machines || [];
        const total = machineList.length;
        const operational = machineList.filter(m => m.status === 'operational').length;
        const warning = machineList.filter(m => m.status === 'warning').length;
        const maintenance = machineList.filter(m => m.status === 'maintenance').length;

        return {
            total,
            operational,
            warning,
            maintenance,
            healthScore: total > 0 ? Math.round((operational / total) * 100) : 0
        };
    }, [machines]);

    return stats;
}

// Custom Hook: useSystemClock
// Demonstrates Point 1 (useState), Point 2 (useEffect) and Point 5 (Custom Hooks)
export function useSystemClock() {
    const [time, setTime] = useState(new Date());

    useEffect(() => {
        const timer = setInterval(() => {
            setTime(new Date());
        }, 1000);

        // Point 2: Cleanup function
        return () => clearInterval(timer);
    }, []);

    return time.toLocaleTimeString();
}

// Custom Hook: useLiveSearch
// Demonstrates Point 4 (useCallback)
export function useLiveSearch(items, searchKey) {
    const [query, setQuery] = useState('');

    const filteredItems = useMemo(() => {
        if (!query) return items;
        return items.filter(item =>
            item[searchKey].toLowerCase().includes(query.toLowerCase())
        );
    }, [items, query, searchKey]);

    return { query, setQuery, filteredItems };
}
