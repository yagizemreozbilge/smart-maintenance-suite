import { renderHook, act } from '@testing-library/react'
import { describe, it, expect } from 'vitest'
import { MaintenanceProvider, useMaintenance, useMaintenanceDispatch } from '../../../frontend/src/context/MaintenanceContext'

describe('MaintenanceContext', () => {

  it('Default state doğru', () => {
    const wrapper = ({ children }) => (
      <MaintenanceProvider>{children}</MaintenanceProvider>
    )

    const { result } = renderHook(() => useMaintenance(), { wrapper })

    expect(result.current.stock).toBe(0)
    expect(result.current.machines).toEqual([])
  })

  it('ADD_MACHINE çalışıyor', () => {
    const wrapper = ({ children }) => (
      <MaintenanceProvider>{children}</MaintenanceProvider>
    )

    const { result } = renderHook(() => {
      return {
        state: useMaintenance(),
        dispatch: useMaintenanceDispatch()
      }
    }, { wrapper })

    act(() => {
      result.current.dispatch({
        type: 'ADD_MACHINE',
        payload: { id: 1, name: 'Test Machine' }
      })
    })

    expect(result.current.state.machines.length).toBe(1)
  })

  it('REMOVE_MACHINE çalışıyor', () => {
    const wrapper = ({ children }) => (
      <MaintenanceProvider>{children}</MaintenanceProvider>
    )

    const { result } = renderHook(() => {
      return {
        state: useMaintenance(),
        dispatch: useMaintenanceDispatch()
      }
    }, { wrapper })

    act(() => {
      result.current.dispatch({
        type: 'ADD_MACHINE',
        payload: { id: 1 }
      })
    })

    act(() => {
      result.current.dispatch({
        type: 'REMOVE_MACHINE',
        payload: 1
      })
    })

    expect(result.current.state.machines.length).toBe(0)
  })
})