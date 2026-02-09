import { describe, it, expect, beforeEach } from 'vitest';
import { renderHook, act } from '@testing-library/react';
import { MaintenanceProvider, useMaintenance, useMaintenanceDispatch } from '../../../frontend/src/context/MaintenanceContext';

describe('MaintenanceContext', () => {
    it('should provide initial state', () => {
        const { result } = renderHook(() => useMaintenance(), {
            wrapper: MaintenanceProvider
        });

        expect(result.current.machines).toEqual([]);
        expect(result.current.alerts).toEqual([]);
        expect(result.current.inventory).toEqual([]);
        expect(result.current.maintenanceLogs).toEqual([]);
    });

    it('should add machine via dispatch', () => {
        const wrapper = ({ children }) => <MaintenanceProvider>{children}</MaintenanceProvider>;

        const { result: stateResult } = renderHook(() => useMaintenance(), { wrapper });
        const { result: dispatchResult } = renderHook(() => useMaintenanceDispatch(), { wrapper });

        act(() => {
            dispatchResult.current({
                type: 'ADD_MACHINE',
                payload: { id: 1, name: 'Test Machine', status: 'operational' }
            });
        });

        expect(stateResult.current.machines).toHaveLength(1);
        expect(stateResult.current.machines[0].name).toBe('Test Machine');
    });

    it('should set machines array', () => {
        const wrapper = ({ children }) => <MaintenanceProvider>{children}</MaintenanceProvider>;

        const { result: stateResult } = renderHook(() => useMaintenance(), { wrapper });
        const { result: dispatchResult } = renderHook(() => useMaintenanceDispatch(), { wrapper });

        const mockMachines = [
            { id: 1, name: 'Machine 1', status: 'operational' },
            { id: 2, name: 'Machine 2', status: 'maintenance' }
        ];

        act(() => {
            dispatchResult.current({
                type: 'SET_MACHINES',
                payload: mockMachines
            });
        });

        expect(stateResult.current.machines).toHaveLength(2);
        expect(stateResult.current.machines[0].name).toBe('Machine 1');
    });

    it('should set alerts array', () => {
        const wrapper = ({ children }) => <MaintenanceProvider>{children}</MaintenanceProvider>;

        const { result: stateResult } = renderHook(() => useMaintenance(), { wrapper });
        const { result: dispatchResult } = renderHook(() => useMaintenanceDispatch(), { wrapper });

        const mockAlerts = [
            { id: 1, severity: 'CRITICAL', message: 'High temperature' }
        ];

        act(() => {
            dispatchResult.current({
                type: 'SET_ALERTS',
                payload: mockAlerts
            });
        });

        expect(stateResult.current.alerts).toHaveLength(1);
        expect(stateResult.current.alerts[0].severity).toBe('CRITICAL');
    });

    it('should set inventory array', () => {
        const wrapper = ({ children }) => <MaintenanceProvider>{children}</MaintenanceProvider>;

        const { result: stateResult } = renderHook(() => useMaintenance(), { wrapper });
        const { result: dispatchResult } = renderHook(() => useMaintenanceDispatch(), { wrapper });

        const mockInventory = [
            { id: 1, part_name: 'Bearing', quantity: 50, min_stock_level: 10 }
        ];

        act(() => {
            dispatchResult.current({
                type: 'SET_INVENTORY',
                payload: mockInventory
            });
        });

        expect(stateResult.current.inventory).toHaveLength(1);
        expect(stateResult.current.inventory[0].part_name).toBe('Bearing');
    });

    it('should set maintenance logs array', () => {
        const wrapper = ({ children }) => <MaintenanceProvider>{children}</MaintenanceProvider>;

        const { result: stateResult } = renderHook(() => useMaintenance(), { wrapper });
        const { result: dispatchResult } = renderHook(() => useMaintenanceDispatch(), { wrapper });

        const mockLogs = [
            { id: 1, machine_id: 1, performer: 'Tech A', description: 'Routine check' }
        ];

        act(() => {
            dispatchResult.current({
                type: 'SET_MAINTENANCE_LOGS',
                payload: mockLogs
            });
        });

        expect(stateResult.current.maintenanceLogs).toHaveLength(1);
        expect(stateResult.current.maintenanceLogs[0].performer).toBe('Tech A');
    });
});
